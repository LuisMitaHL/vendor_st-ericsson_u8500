#ifndef _CONVENIENCE_H_
#define _CONVENIENCE_H_

/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/

#include <stddef.h>

/**********************************************
 * Macros
 **********************************************/

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#define ELEMENTS(a)	(sizeof(a) / sizeof((a)[0]))

#define NOTUSED(x) (void)x

/**********************************************
 * Functions
 **********************************************/

void *easy_malloc(size_t size);
void *easy_calloc(size_t members, size_t size);
void *easy_realloc(void *ptr, size_t size);
void easy_free(void *ptr);
char *easy_strdup(const char *s);

#endif // _CONVENIENCE_H_

