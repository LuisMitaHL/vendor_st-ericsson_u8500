/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   queue.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdbool.h>
#include "dbc.h"

typedef void *queue_item_t;

typedef struct {
    volatile queue_item_t *circ_array;
    unsigned int           full  : 1;
    unsigned int           size  : 7;
    volatile unsigned int  front : 8;
    volatile unsigned int  count : 8;
    bool                   enabled;
} Queue_t;

void         Queue_init(Queue_t *, void *buf, int size, bool full, bool enabled);
void         Queue_resetCount(Queue_t *);
queue_item_t Queue_getItem(const Queue_t *, int i);
void Queue_setEnabled(Queue_t * this, bool enabled);
bool Queue_getEnabled(const Queue_t *this);

queue_item_t Queue_pop_front(Queue_t *);
void Queue_push_back(Queue_t *, queue_item_t);
void Queue_push_front(Queue_t *, queue_item_t);

int  Queue_size(const Queue_t *);
int  Queue_itemCount(const Queue_t *);
bool Queue_empty(const Queue_t *);
