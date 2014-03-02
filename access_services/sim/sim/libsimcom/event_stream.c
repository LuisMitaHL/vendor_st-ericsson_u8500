/*                               -*- Mode: C -*- 
 * Copyright (C) 2009, ST-Ericsson
 * $Id$
 * 
 * File name       : event_stream.c
 * Description     : 
 * 
 * Author          : Mats Bergstrom
 * Created On      : Mon Oct 26 08:10:33 2009
 * 
 * Last Modified By: 
 * Last Modified On: Fri Apr 16 14:29:02 2010
 * Update Count    : 47
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "event_stream.h"
#include "sim_unused.h"



typedef struct {
    char                   *buf_lo;     /* points at start of data */
    char                   *buf_hi;     /* points at end of data + 1 */
    char                   *buf;        /* points to the buffer */
    char                   *buf_max;    /* points at top of buffer + 1 */
} ste_buf_t;



ste_buf_t              *ste_buf_new(size_t bsize)
{
    ste_buf_t              *me = malloc(sizeof(ste_buf_t));
    if (me) {
        me->buf = malloc(bsize);
        if (me->buf) {
            me->buf_lo = me->buf;
            me->buf_hi = me->buf;
            me->buf_max = me->buf;
            me->buf_max += bsize;
        } else {
            free(me);
            me = 0;
        }
    }
    return me;
}

void ste_buf_delete(ste_buf_t * me)
{
    free(me->buf);
    free(me);
}


enum ste_es_kind_e {
    ste_es_kind_normal = 0,     /* Normal */
    ste_es_kind_accept,         /* Accept socket */
    ste_es_kind_nonbuf          /* Read calls parse cb. */
};
typedef enum ste_es_kind_e ste_es_kind_t;


struct ste_es_s {
    ste_es_kind_t           es_kind;
    int                     fd; /* File descritpor */
    ste_buf_t              *ib; /* Input buffer */
    int                     accepted_fd;        /* from accept() */
    ste_es_state_t          state;      /* State of stream */
    ste_es_parser_closure_t pc; /* Closure for parsing */
};



static                  ssize_t
ste_es_parser_func_noop(char *buf, char *buf_max, void *UNUSED(ud))
{
    return buf_max - buf;
}

static                  ssize_t
ste_es_parser_func_noop_non_buf(char *UNUSED(buf),
                                char *UNUSED(buf_max),
                                void *UNUSED(ud))
{
    return 1;
}

ste_es_t               *ste_es_new_normal(int fd, size_t bsize,
                                          ste_es_parser_closure_t * pc)
{
    ste_es_t               *me = malloc(sizeof(ste_es_t));
    if (me) {
        me->es_kind = ste_es_kind_normal;
        me->fd = fd;
        me->ib = ste_buf_new(bsize);
        me->accepted_fd = -1;
        me->state = STE_ES_STATE_OPEN;
        if (pc) {
            me->pc = *pc;
            if (!me->pc.func) {
                me->pc.func = ste_es_parser_func_noop;
            }
        } else {
            me->pc.func = ste_es_parser_func_noop;
            me->pc.user_data = 0;
        }
    }
    return me;
}


ste_es_t               *ste_es_new_accept(int fd,
                                          ste_es_parser_closure_t * pc)
{
    ste_es_t               *me = malloc(sizeof(ste_es_t));
    if (me) {
        me->es_kind = ste_es_kind_accept;
        me->fd = fd;
        me->ib = 0;
        me->accepted_fd = -1;
        me->state = STE_ES_STATE_OPEN;
        if (pc) {
            me->pc = *pc;
            if (!me->pc.func) {
                me->pc.func = ste_es_parser_func_noop;
            }
        } else {
            me->pc.func = ste_es_parser_func_noop;
            me->pc.user_data = 0;
        }
    }
    return me;
}


ste_es_t               *ste_es_new_nonbuf(int fd,
                                          ste_es_parser_closure_t * pc)
{
    ste_es_t               *me = malloc(sizeof(ste_es_t));
    if (me) {
        me->es_kind = ste_es_kind_nonbuf;
        me->fd = fd;
        me->ib = 0;
        me->accepted_fd = -1;
        me->state = STE_ES_STATE_OPEN;
        if (pc) {
            me->pc = *pc;
            if (!me->pc.func) {
                me->pc.func = ste_es_parser_func_noop_non_buf;
            }
        } else {
            me->pc.func = ste_es_parser_func_noop_non_buf;
            me->pc.user_data = 0;
        }
    }
    return me;
}


void ste_es_delete(ste_es_t * me)
{
    if (me) {
        /* Make callback with buffer set to zero to indicate that the event stream is deleted */
        me->pc.func(0, 0, me->pc.user_data);
        if (me->ib) {
            ste_buf_delete(me->ib);
        }
        free(me);
    }
}

void ste_es_set_ud(ste_es_t *es, void *user_data)
{
    es->pc.user_data = user_data;
}

int ste_es_fd(ste_es_t * es)
{
    return es->fd;
}



ste_es_state_t ste_es_state(ste_es_t * es)
{
    return es->state;
}




static ssize_t ste_es_i_read(ste_es_t * es)
{
    ste_buf_t              *buf = es->ib;
    size_t                  n_max;
    ssize_t                 n;


    /* Normal operation, */
    if (es->state == STE_ES_STATE_OPEN) {

        if (ioctl(es->fd, FIONREAD, &n) >= 0) {
            /* Re-allocate the buffer to a new max size if the old size is insufficient*/
            if (n > (buf->buf_max - buf->buf_lo)) {
                ssize_t r_lo = buf->buf_lo - buf->buf;
                ssize_t r_hi = buf->buf_hi - buf->buf;

                buf->buf = realloc(buf->buf, n);
                buf->buf_lo = buf->buf + r_lo;
                buf->buf_hi = buf->buf + r_hi;
                buf->buf_max = buf->buf + n;
            }
        }

        /* If buffer is empty move pointer to start of buffer */
        if (buf->buf_hi == buf->buf_lo) {
            buf->buf_lo = buf->buf;
            buf->buf_hi = buf->buf;
        }
        /* If there is unused space at start of buffer, move data */
        else if (buf->buf_lo > buf->buf) {
            char                   *dst = buf->buf;
            char                   *src = buf->buf_lo;
            char                   *top = buf->buf_hi;
            while (src < top) {
                *dst = *src;
                ++dst;
                ++src;
            }
            buf->buf_lo = buf->buf;
            buf->buf_hi = dst;
        }

        /* Max bytes to read */
        n_max = buf->buf_max - buf->buf_hi;

        if (n_max > 0) {
            n = read(es->fd, buf->buf_hi, n_max);
            if (n > 0) {
                /* OK */
                buf->buf_hi += n;
                n = buf->buf_hi - buf->buf_lo;
            } else if (n == 0) {
                /* EOF */
                es->state = STE_ES_STATE_ATEOF;
                n = buf->buf_hi - buf->buf_lo;
            } else {
                /* Error */
                int                     err = errno;
                switch (err) {
                case EAGAIN:
                    /* No error, just normal operation. */
                    n = buf->buf_hi - buf->buf_lo;
                    break;

                case EINTR:
                    /* Interrupted by a signal, no real error */
                    n = buf->buf_hi - buf->buf_lo;
                    break;

                default:
                    /* Real errors */
                    es->state = STE_ES_STATE_ERROR;
                    break;
                }

            }
        }
        else {
            // No of bytes in buffer.
            n = buf->buf_hi - buf->buf_lo;
        }
    }

    else if (es->state == STE_ES_STATE_ATEOF) {
        n = buf->buf_hi - buf->buf_lo;
    }

    else if (es->state == STE_ES_STATE_ERROR) {
        n = -1;
    }

    else {
        n = -1;
    }

    return n;
}


static ssize_t ste_es_i_accept(ste_es_t * es)
{
    int                     s;
    struct sockaddr_un      addr;
    socklen_t               addrlen = sizeof(struct sockaddr_un);
    int                     i;
    ssize_t                 rv = 0;

    /* Normal operation, */
    if (es->state == STE_ES_STATE_OPEN) {

        s = accept(es->fd, (struct sockaddr *) &addr, &addrlen);
        if (s > 0) {
            /* OK */

            i = fcntl(s, F_SETFL, O_NONBLOCK);
            if (i < 0) {
                close(s);
            } else {
                es->accepted_fd = s;
                rv = 1;
            }
        } else {
            /* Error */
            int                     err = errno;
            switch (err) {
            case EAGAIN:
                /* No error, just normal operation. */
                break;

            case EINTR:
                break;

            default:
                /* Real errors */
                es->state = STE_ES_STATE_ERROR;
                rv = -1;
                break;
            }
        }
    }

    return rv;
}



static ssize_t ste_es_i_nonbuf(ste_es_t * es)
{
    ssize_t                 rv = 0;
    char                   *ptr;
    ssize_t                 n;
    ptr = (char *) &(es->fd);
    n = (es->pc.func) (ptr, 0, es->pc.user_data);
    if (n < 0) {
        es->state = STE_ES_STATE_ERROR;
    } else if ( n == 2 ) {
        es->state = STE_ES_STATE_RESET;
        rv = 1;
    } else {
        rv = 1;
    }
    return rv;
}



ssize_t ste_es_read(ste_es_t * es)
{
    ssize_t                 n;
    switch (es->es_kind) {
    case ste_es_kind_normal:
        n = ste_es_i_read(es);
        break;
    case ste_es_kind_accept:
        n = ste_es_i_accept(es);
        break;
    case ste_es_kind_nonbuf:
        n = ste_es_i_nonbuf(es);
        break;
    default:
        n = -1;
    }
    return n;
}



static ssize_t ste_es_i_read_parse(ste_es_t * es)
{
    ste_buf_t              *buf = es->ib;
    ssize_t                 rv = 0;     /* No of successful parsings */

    while (buf->buf_lo < buf->buf_hi) {
        ssize_t                 n =
            (es->pc.func) (buf->buf_lo, buf->buf_hi, es->pc.user_data);
        if (n > 0) {
            /* Normal success */
            buf->buf_lo += n;
            if (buf->buf_lo > buf->buf_hi) {
                /* FIXME: Warn here? */
                buf->buf_lo = buf->buf_hi;
            }
            rv += n;
        } else if (n == 0) {
            /* Normal parse failure, just need more data. */
            break;
        } else if (n < 0) {
            /* Error, irrecoverable parse error */
            es->state = STE_ES_STATE_ERROR;
            rv = -1;
            break;
        }
    }

    return rv;
}


static ssize_t ste_es_i_accept_parse(ste_es_t * es)
{
    ssize_t                 rv = 0;

    if (es->accepted_fd >= 0) {
        char                   *ptr = (char *) &(es->accepted_fd);
        ssize_t                 n =
            (es->pc.func) (ptr, 0, es->pc.user_data);
        es->accepted_fd = -1;
        if (n < 0) {
            es->state = STE_ES_STATE_ERROR;
        } else {
            rv = 1;
        }
    }

    return rv;
}

ssize_t ste_es_parse(ste_es_t * es)
{
    ssize_t                 n;
    switch (es->es_kind) {
    case ste_es_kind_normal:
        n = ste_es_i_read_parse(es);
        break;
    case ste_es_kind_accept:
        n = ste_es_i_accept_parse(es);
        break;
    case ste_es_kind_nonbuf:
        n = 1;
        break;
    default:
        n = -1;
    }
    return n;
}
