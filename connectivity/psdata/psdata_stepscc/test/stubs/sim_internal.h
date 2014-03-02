/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : sim_internal.h
 * Description     : sim stub
 *
 * Author          : Christian Nilsson <christian.c.nilsson@stericsson.com>
 *
 */

#include <pthread.h>


typedef enum {
    ste_sim_state_disconnected,
    ste_sim_state_connected
} ste_sim_lib_state_t;

typedef struct {
    uint16_t    expected_response_id;    /* What we wait for */
    void*       vdata;                   /* Response dependent data */
    int         is_sync;                 /* 1/0 for sync/async */
    int         done;                    /* set to 1 by es cb func */
    int         rv;                      /* Return value */
} ste_sim_sync_data_t;

/* Info interface needs about the client */
struct ste_sim_s {
    ste_sim_lib_state_t     state;          /* Connection state; on or off */
    ste_sim_closure_t       closure;        /* Closure for call back */
    int                     thread_pipe;    /* -1 if no thread is running */
    int                     fd;             /* Socket file descriptor */
    int                     is_st;          /* 0/1 if we are single threaded */
    pthread_t               rdr_tid;        /* Reader thread (-1 if not connected in multi-thread mode) */
    int                *es;            /* Event stream used in single threaded mode */
    ste_sim_sync_data_t     sync_data;      /* Used in sync mode */
};
