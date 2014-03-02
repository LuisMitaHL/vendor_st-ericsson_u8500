/**
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

int printf(const char *fmt, ...)
                    __attribute__ ((__format__ (__printf__, 1, 2)));
int snprintf(char *str, size_t size, const char *fmt, ...)
                    __attribute__ ((__format__ (__printf__, 3, 4)));
int vsnprintf (char *str, size_t size, const char *fmt, va_list ap)
                    __attribute__ ((__format__ (__printf__, 3, 0)));

int puts(const char *str);

#endif /*STDIO_H*/
