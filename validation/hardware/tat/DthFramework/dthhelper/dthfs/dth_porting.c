/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH filesystem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "dth_porting.h"


inline void *dth_malloc(unsigned long size)
{
	return malloc(size) ;
}

inline void dth_free(void *ptr)
{
	if (ptr != NULL)
		free(ptr) ;
}

inline unsigned long dth_strlen(const char *string)
{
	return strlen(string);
}

inline int dth_strncmp(const char *s1, const char *s2, unsigned long size)
{
	return strncmp(s1, s2, size) ;
}

inline unsigned long dth_get_time(void)
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec ;
}

