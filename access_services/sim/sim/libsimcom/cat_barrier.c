/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : cat_barrier.c
 * Description     : Barrier object for thread rendevous
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */


#include <sys/time.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#include "cat_barrier.h"

struct cat_barrier_s {
    pthread_mutex_t         mtx;
    pthread_cond_t          cv;
    int                     is_set;
    void                   *data;
};

cat_barrier_t          *cat_barrier_new()
{
    cat_barrier_t          *p = malloc(sizeof(cat_barrier_t));
    if (p) {
        pthread_mutex_init(&(p->mtx), 0);
        pthread_cond_init(&(p->cv), 0);
        p->is_set = 0;
        p->data = 0;
    }
    return p;
}



void cat_barrier_delete(cat_barrier_t * p)
{
    if (p) {
        pthread_mutex_destroy(&(p->mtx));
        pthread_cond_destroy(&(p->cv));
        free(p);
    }
}


int cat_barrier_set(cat_barrier_t * p)
{
    pthread_mutex_lock(&(p->mtx));
    do {
        p->data = 0;
        if (!p->is_set) {
            p->is_set = 1;
            pthread_cond_signal(&(p->cv));
        }
    } while (0);
    pthread_mutex_unlock(&(p->mtx));
    return 0;
}


int cat_barrier_release(cat_barrier_t * p, void *data)
{
    if (pthread_mutex_lock(&(p->mtx)) != 0) {
        return -1;
    }
    do {
        while (!(p->is_set)) {
            if (pthread_cond_wait(&(p->cv), &(p->mtx)) != 0) {
                pthread_mutex_unlock(&(p->mtx));
                return -1;
            }
        }
        p->is_set = 0;
        p->data = data;
        pthread_cond_signal(&(p->cv));
    } while (0);
    pthread_mutex_unlock(&(p->mtx));
    return 0;
}


int cat_barrier_wait(cat_barrier_t * p, void **hdata)
{
    int res;
    if (pthread_mutex_lock(&(p->mtx)) != 0) {
        return -1;
    }
    do {
        while ((p->is_set)) {
            if (pthread_cond_wait(&(p->cv), &(p->mtx)) != 0) {
                pthread_mutex_unlock(&(p->mtx));
                return -1;
            }
        }
        if (hdata)
            *hdata = p->data;
        p->data = 0;
    } while (0);
    pthread_mutex_unlock(&(p->mtx));
    return 0;
}


int cat_barrier_timedwait(cat_barrier_t * p, void **hdata, unsigned ms)
{
    int                     rv = 0;

    if (pthread_mutex_lock(&(p->mtx)) != 0) {
        return -1;
    }
    do {
        int                     i = 0;
        struct timespec         ts;
        struct timeval          now;
        unsigned                sec = ms / 1000;
        ms -= sec * 1000;

        gettimeofday(&now, NULL);

        now.tv_sec += sec;
        now.tv_usec += ms * 1000;
        if (now.tv_usec > 1000000) {
            now.tv_usec -= 1000000;
            now.tv_sec += 1;
        }
        ts.tv_sec = now.tv_sec;
        ts.tv_nsec = now.tv_usec * 1000;

        while ((p->is_set)) {
            i = pthread_cond_timedwait(&(p->cv), &(p->mtx), &ts);
            if (i == ETIMEDOUT) {
                rv = 1;
                goto Done;
            } else if (i != 0) {
                rv = -1;
                goto Done;
            }
        }
        if (hdata)
            *hdata = p->data;
        p->data = 0;
    } while (0);
  Done:
    pthread_mutex_unlock(&(p->mtx));
    return rv;
}

