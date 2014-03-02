/*-----------------------------------------------------------------------------
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: jens.wiklander@stericsson.com
 *---------------------------------------------------------------------------*/

/*
 * This file provides what C99 standard requires for <string.h>
 * for some functions
 */

#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <cdefs.h>

void *memcpy(void * __restrict s1, const void *__restrict s2, size_t n);
void *memmove(void *s1, const void *s2, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *s, int c, size_t n);
void *memchr(const void *s, int c, size_t n);

int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);

size_t strlcpy(char *dst, const char *src, size_t size);
size_t strlcat(char *dst, const char *src, size_t size);

#endif /*STRING_H*/
