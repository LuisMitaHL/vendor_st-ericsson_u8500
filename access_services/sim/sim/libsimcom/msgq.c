/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : msgq.c
 * Description     : Message queue and base message implementation file.
 *  The implementation is thread safe under the following assumption:
 *  There is a server thread that owns the queue, i.e. creates it,
 *  pops items from it for
 *  processing, and finally deletes the queue.
 *  Other threads, including the server thread, are allowed to push items.
 *
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 *                   Stefan Svenberg
 */

#include "msgq.h"

#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef TRUE
#define TRUE (!0)
#endif
#ifndef FALSE
#define FALSE (!TRUE)
#endif

/*
 * This is the list cell used to store a pointer to the message, and a
 * pointer to next element (NULL if end of list).
 */
typedef struct ste_msgh_s {
  struct ste_msgh_s *next;
   ste_msg_t  *msg;
} ste_msgh_t;

/*
 * The queue consists of the list and a tail pointer that points to the last
 * list pointer of the list (hence double pointer). The following always holds:
 * tail always points to a NULL pointer
 * if the head is NULL, then tail points to the head.
 */
struct ste_msgq_s {
  ste_msgh_t *head;
  ste_msgh_t **tail; // Points to the next-pointer of last item in queue.

  pthread_mutex_t mtx;
  pthread_cond_t  cv;
};

/**
 * Internal static primitives to work directly on the queue data structure
 */

/*
 * This removes a list msg cell pointed to by h from the list.
 * To remove the head, unlink(q, &q->head).
 * If the unlink could not be done, then NULL is returned, for example
 * if h is at the end of the list.
 */
static ste_msg_t *unlink_msg(ste_msgq_t *q, ste_msgh_t **h) {
  ste_msg_t *msg;
  ste_msgh_t *hh;

  if ( !q || !h || !*h ) {
      return NULL;
  }
  hh = *h;
  msg = hh->msg;
  *h = hh->next;
  if ( !*h ) {
      q->tail = h;
  }
  free(hh);
  return msg;
}

// Links a new msg at link h. Return true if it could be inserted,
// false otherwise.
static int link_msg(ste_msgq_t *q, ste_msgh_t **h, ste_msg_t *msg) {
  ste_msgh_t *hh;

  if ( !q || !h ) {
      return 0;
  }
  hh = malloc(sizeof(ste_msgh_t));
  if ( !hh ) {
      return 0;
  }
  hh->msg = msg;
  hh->next = *h;
  *h = hh;
  if ( !hh->next ) {
      q->tail = &hh->next;
  }
  return 1;
}

/*
 * Tests if a give msg type matches the filter. NULL is returned if no
 * match is found. The search in the int filter array is stopped when zero
 * is found.
 */
static int ste_msgq_match(int type, const int* filter)
{
  while ( *filter >= 0 ) {
    if ( type == *filter ) {
      return 1;
    }
    ++filter;
  }
  return 0;
}

/*
 * Returns a pointer to the first pointer to the cell (in the order from head
 * to tail) in the queue which has a msg with a type that occurs in the filter.
 * If no match is found, then NULL is returned.
 * The filter must be a zero terminated array of int. Msg type is int.
 *
 * NOTE: Returning a double pointer makes it easy to call unlink msg on
 * the result.
 */
static ste_msgh_t **find_msgq_match(ste_msgq_t *q, const int *filter) {
  ste_msgh_t **hh;
  for ( hh = &q->head; *hh; hh = &(*hh)->next ) {
    if ( ste_msgq_match((*hh)->msg->type, filter) ) {
      return hh;
    }
  }
  return NULL;
}

/*
 * Delete a ste message.
 */
void ste_msg_delete(ste_msg_t * msg)
{
  if ( msg ) {
    if ( msg->delete_func ) {
      msg->delete_func(msg);
    } else {
      free(msg);
    }
  }
}

/*
 * Create and initialize a new msg queue. The tail is set to point to the head
 * pointer, reflecting the state when the queue is empty.
 */
ste_msgq_t *ste_msgq_new()
{
  ste_msgq_t *q = malloc(sizeof(ste_msgq_t));
  if ( q ) {
    if ( pthread_mutex_init(&q->mtx, 0) != 0) { /* 0=default args */
        free(q);
        return NULL;
    }
    if ( pthread_cond_init(&q->cv, 0) != 0 ) { /* 0=default args */
        pthread_mutex_destroy(&q->mtx);
        free(q);
        return NULL;
    }
    q->head = 0;
    q->tail = &q->head;
  }
  return q;
}

void ste_msgq_delete(ste_msgq_t *q)
{
  ste_msg_t *msg;

  if ( !q ) {
      return;
  }
  if ( pthread_mutex_lock(&q->mtx) != 0 ) {
      return;
  }
  while ( q->head ) {
    msg = unlink_msg(q, &q->head);
    ste_msg_delete(msg);
  }
  pthread_cond_destroy(&q->cv);
  pthread_mutex_unlock(&q->mtx);
  pthread_mutex_destroy(&q->mtx);
  free(q);
}

/*
 * Internal implementation to add a new message to the queue.
 *
 * The message is either linked at the head or the tail of the queue
 * depending on the value of the dopush parameter.
 *
 * If the message cannot be pushed, then the message is lost.
 *
 * @param
 *          q       pointer to the queue
 *          msg     pointer to the message to add to the queue
 *          dopush  set to TRUE to add message at the head, or FALSE to add message at the tail.
 */
void ste_msgq_internal_add(ste_msgq_t *q, ste_msg_t *msg, int dopush)
{
    if ( q ) {
        ste_msgh_t** linkref = NULL;
        if ( pthread_mutex_lock(&q->mtx) != 0 ) {
            return;
        }
        linkref = ((dopush) ? &q->head : q->tail);
        if ( !link_msg(q, linkref, msg) ) {
            ste_msg_delete(msg);
        }
        pthread_mutex_unlock(&q->mtx);
        pthread_cond_signal(&q->cv);
    } else {
        ste_msg_delete(msg);
    }
}

void ste_msgq_push(ste_msgq_t *q, ste_msg_t *msg)
{
    ste_msgq_internal_add(q, msg, TRUE);
}

void ste_msgq_add(ste_msgq_t *q, ste_msg_t *msg)
{
    ste_msgq_internal_add(q, msg, FALSE);
}

/*
 * Pop msg from queue. The msg is unlinked from the head position.
 */
ste_msg_t *ste_msgq_pop(ste_msgq_t *q)
{
  ste_msg_t *msg;

  if ( !q ) {
      return NULL;
  }
  if ( pthread_mutex_lock(&q->mtx) != 0 ) {
      return NULL;
  }
  while (!q->head) {
    if ( pthread_cond_wait(&q->cv, &q->mtx) != 0 ) {
        pthread_mutex_unlock(&q->mtx);
        return NULL;
    }
  }
  msg = unlink_msg(q, &q->head);
  pthread_mutex_unlock(&q->mtx);
  return msg;
}

/*
 * Searches through the msgq for a msg where the type is allowed from the
 * filter. The filter is a null-terminated array of int.
 */
ste_msg_t *ste_msgq_pop_filtered(ste_msgq_t *q, const int *filter)
{
  ste_msgh_t **h;
  ste_msg_t *msg;

  if ( !q || !filter ) {
      return NULL;
  }
  if ( pthread_mutex_lock(&q->mtx) != 0 ) {
      return NULL;
  }
  h = find_msgq_match(q, filter);
  while ( !h ) {
    if ( pthread_cond_wait(&q->cv, &q->mtx) != 0 ) {
        pthread_mutex_unlock(&q->mtx);
        return NULL;
    }
    h = find_msgq_match(q, filter);
  }
  msg = unlink_msg(q, h);
  pthread_mutex_unlock(&q->mtx);
  return msg;
}

/*
 * Pop the queue if it is non-empty. If it is empty then return NULL.
 */
ste_msg_t *ste_msgq_try_pop(ste_msgq_t *q)
{
  ste_msg_t *msg;

  if ( !q ) {
      return NULL;
  }
  if ( pthread_mutex_lock(&q->mtx) != 0 ) {
      return NULL;
  }
  msg = unlink_msg(q, &q->head); // Returns NULL if q is empty
  pthread_mutex_unlock(&q->mtx);
  return msg;
}

/*
 * If the queue is empty, a msg must arrive within ms milliseconds to be
 * returned by this fn. Otherwise the function returns NULL.
 */
ste_msg_t *ste_msgq_timed_pop(ste_msgq_t * q, unsigned ms)
{
  ste_msg_t              *msg = 0;

  int                     i = 0;
  struct timespec         ts;
  struct timeval          now;
  unsigned                sec = ms / 1000;

  if ( !q ) {
      return NULL;
  }
  ms -= sec * 1000;

  gettimeofday(&now, NULL);

  now.tv_sec += sec;
  now.tv_usec += ms * 1000;
  if ( now.tv_usec > 1000000 ) {
    now.tv_usec -= 1000000;
    now.tv_sec += 1;
  }
  ts.tv_sec = now.tv_sec;
  ts.tv_nsec = now.tv_usec * 1000;

  if ( pthread_mutex_lock(&q->mtx) != 0 ) {
      return NULL;
  }

  if ( q->head ) {
    msg = unlink_msg(q, &q->head);
  } else {
    while ( !q->head ) {
      i = pthread_cond_timedwait(&q->cv, &q->mtx, &ts);
      if ( i ) {
        break;
      }
    }
    msg = unlink_msg(q, &q->head);
  }
  pthread_mutex_unlock(&q->mtx);
  return msg;
}


ste_msg_void_t         *ste_msg_void_create(int type, uintptr_t client_tag)
{
    ste_msg_void_t         *p;
    p = malloc(sizeof(*p));
    if ( p ) {
        p->type = type;
        p->delete_func = 0;
        p->client_tag = client_tag;
    }
    return p;
}
