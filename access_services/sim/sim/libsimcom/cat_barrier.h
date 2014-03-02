/*                               -*- Mode: C -*- 
 * Copyright (C) 2010, ST-Ericsson
 * 
 * File name       : cat_barrier.h
 * Description     : Barrier, thread sync.
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */

// FIXME: Rename cat_barrier --> ste_barrier
// FIXME: DOXYGEN!

#ifndef __cat_barrier_h__
#define __cat_barrier_h__ (1)

// A barrier used for syncronisation between threads.
// Usage:
//      thread 1:
//              bar = cat_barrier_new();
//              cat_barrier_set(bar);
//              tell_thread_1_about(bar);
//              void* return_value = 0;
//              if ( cat_barrier_wait(&return_value) != 0 )
//                abort();
//              handle_return_value(return_value);
//              cat_barrier_delete(bar);
//      thread 2:
//              bar = get_from_thread_1();
//              my_return_value = something;
//              cat_barrier_release(bar);

typedef struct cat_barrier_s cat_barrier_t;
cat_barrier_t          *cat_barrier_new();
void                    cat_barrier_delete(cat_barrier_t *);
int                     cat_barrier_set(cat_barrier_t *);
int                     cat_barrier_release(cat_barrier_t *, void *);
int                     cat_barrier_wait(cat_barrier_t *, void **);
int                     cat_barrier_timedwait(cat_barrier_t *, void **,
                                              unsigned ms);


#endif
