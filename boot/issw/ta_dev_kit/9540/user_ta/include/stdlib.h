/*-----------------------------------------------------------------------------
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *---------------------------------------------------------------------------*/

/*
 * This file provides what C99 standard requires for <stdlib.h> in
 * 7.20.3 Memory management functions
 */

#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

/*
 * Adds a pool of memory to allocate from, can currently only be called
 * once due to limitation in malloc_buffer_is_within_alloced()
 */
void malloc_add_heap(void *buf, size_t len);

/*
 * Returns true if the supplied memory area is within a buffer
 * previously allocated (and not freed yet).
 */
bool malloc_buffer_is_within_alloced(void *buf, size_t len);

/*
 * Returns true if the supplied memory area is overlapping the area used
 * for heap.
 */
bool malloc_buffer_overlaps_heap(void *buf, size_t len);


/*
 * Calls allocates a buffer as described in TEE Internal API
 * with flags to indicate if malloced memory should be cleared
 * or not before return.
 */
void *malloc_with_flags(size_t size, uint32_t flags);


void abort(void);

#endif /*STDLIB_H*/
