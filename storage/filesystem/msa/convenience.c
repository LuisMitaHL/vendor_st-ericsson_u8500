/************************************************************************
 *                                                                      *
 *  Copyright (C) 2010 ST-Ericsson                                      *
 *                                                                      *
 *  Author: Joakim AXELSSON <joakim.axelsson AT stericsson.com>         *
 *  Author: Sebastian RASMUSSEN <sebastian.rasmussen AT stericsson.com> *
 *                                                                      *
 ************************************************************************/


#include "convenience.h"

#include "likely.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

void *easy_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (unlikely(!ptr))
	{
		logFATAL("Can not allocate memory!");
		abort();
	}
	return ptr;
}

void *easy_calloc(size_t members, size_t size)
{
	void *ptr = calloc(members, size);
	if (unlikely(!ptr))
	{
		logFATAL("Can not allocate and clear memory!");
		abort();
	}
	return ptr;
}

void *easy_realloc(void *ptr, size_t size)
{
	void *newptr = realloc(ptr, size);
	if (unlikely(!newptr))
	{
		logFATAL("Can not resize memory allocation!");
		abort();
	}
	return newptr;
}

void easy_free(void *ptr)
{
	free(ptr);
}

char *easy_strdup(const char *s)
{
	char *cs = strdup(s);
	if (unlikely(!cs))
	{
		logFATAL("Can not allocate memory for string");
		abort();
	}
	return cs;
}

