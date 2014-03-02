/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : cat_internal.c
 * Description     : sim utility functions.
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */



#include "cat_internal.h"
#include "sim_internal.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>







int cat_launch_thread(pthread_t * tid, void *(*func) (void *), void *arg)
{
    pthread_attr_t          attr;
    int                     i;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    i = pthread_create(tid, &attr, func, arg);
    if (i < 0) {
        perror("failed to create thread");
    }

    pthread_attr_destroy(&attr);


    return i;
}



int ste_cat_i_create_pipe(int *wfs, int *rfs)
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
