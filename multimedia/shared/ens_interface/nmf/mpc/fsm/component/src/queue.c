/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   queue.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "fsm/component.nmf"

#include "inc/archi-wrapper.h"

/* WARNING :
 * Assumptions for fifo synchronization are:
 *  - One reader and one writer
 *  - Init and reset are not concurrent with others functions
 */

/* SYNCHRONIZATION NOTE :
 *  - 'count' is volatile, because it is modified by reader and writer.
 *  - 'front' and 'circ_array' are volatile to get a kind of memory barrier.
 *  - Access to 'front' and 'count' are done in critial sections because they
 *  have a semantical dependency.
 */

void Queue_init(Queue_t *this, void *buf, int size, bool full, bool enabled) {
    PRECONDITION(size  < 127);

    this->circ_array = buf;
    this->size       = size;
    this->front      = 0;
    if (full) {
        this->count = size;
        this->full  = 1;
    } else {
        this->count = 0;
        this->full  = 0;
    }
    this->enabled = enabled;
}

void Queue_setEnabled(Queue_t *this, bool enabled) {
    this->enabled = enabled;
}

bool Queue_getEnabled(const Queue_t *this) {
    return this->enabled;
}

int Queue_size(const Queue_t *this) {
    return this->size;
}

int Queue_itemCount(const Queue_t *this) {
    if (this->enabled) {
        return this->count;
    } else {
        return 0;
    }
}

bool Queue_empty(const Queue_t *q) {
    return Queue_itemCount(q) == 0;
}

queue_item_t Queue_getItem(const Queue_t *this, int i) {

    int idx;

    PRECONDITION(i < Queue_itemCount(this));

    idx = this->front + i;

    if (idx >= this->size) {
        idx -= this->size;
    }

    return this->circ_array[idx];
}

queue_item_t Queue_pop_front(Queue_t *this) {
    queue_item_t front;
    unsigned int read_index;
    PRECONDITION(!Queue_empty(this));

    ENTER_CRITICAL_SECTION;
    read_index = this->front;  // local copy of the volatile variable

    front = this->circ_array[read_index];
    read_index++;
    if (read_index == this->size) {
        read_index = 0;
    }
    this->front = read_index; // update volatile variable
    this->count--;
    EXIT_CRITICAL_SECTION;

    return front;
}

void Queue_push_back(Queue_t *this, queue_item_t item) {
    int          write_index;
    unsigned int count;

    PRECONDITION(Queue_itemCount(this) < this->size);
    ENTER_CRITICAL_SECTION;

    count       = this->count; //local copy of the volatile variable
    write_index = this->front + count;

    if (write_index >= this->size) {
        write_index -= this->size;
    }

    this->circ_array[write_index] = item;

    this->count = count + 1;  // update volatile variable
    EXIT_CRITICAL_SECTION;
}

void Queue_push_front(Queue_t *this, queue_item_t item) {
    int          write_index;
    unsigned int count;

    PRECONDITION(Queue_itemCount(this) < this->size);
    ENTER_CRITICAL_SECTION;

    write_index = this->front - 1;
    if (write_index < 0) {
        write_index += this->size;
    }

    this->circ_array[write_index] = item;

    this->front = write_index;     // update volatile variables
    this->count++;
    EXIT_CRITICAL_SECTION;
}
