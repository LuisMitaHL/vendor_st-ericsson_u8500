/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifdef _NMF_MPC_
#include <libeffects/mpc/libvector.nmf>
#endif //#ifdef _NMF_MPC_
#include "vector.h"

/* x mem variable */
static int      total_char;
static int      max_memory;
static char    *heap;

/* y mem variable */
static int      total_char_y;
static int      max_memory_y;
static YMEM char *heap_y;

/* extern variable */
static int      total_char_extern;
static int      max_memory_extern;
static EXTERN char    *heap_extern;

/* esram variable */
static int      total_char_esram;
static int      max_memory_esram;
#ifdef __flexcc2__ 
static ESRAM char    *heap_esram;
#else
static EXTERN char   *heap_esram;
#endif

/***********************/
/* Allocation in X mem */

PRAGMA_FORCE_DCUMODE
void buffer_init_calloc(char *adr, int size_mem)
{
	if (adr != NULL) {
		heap = adr;
		max_memory = size_mem;
		total_char = 0;
	}
}											/* end init_calloc() */


PRAGMA_FORCE_DCUMODE
void *buffer_calloc(int num, int size)
{
#if 1										//def NOMALLOC
	char           *base;
	int             size_n = size * num;

	/* always align on integer boundary */
	while ((unsigned int) total_char % sizeof (int) != 0) {
		heap++;
		total_char++;
	}

	total_char += size_n;
	if (total_char > max_memory) {
		base = NULL;
	} else {
		base = heap;
		buffer_reset((char *) base, 0, size_n);
	}
	heap += size_n;


	return base;
#else
	extern void    *calloc(int, int);

	/* keep good old calloc for debugging */
	char           *temp;

	temp = calloc(num, size);
	buffer_reset((char *) temp, 0, size * num);
	return temp;
#endif /* NOMALLOC */

}											/* end buffer_calloc() */


/***********************/
/* Allocation in Y mem */

PRAGMA_FORCE_DCUMODE
void buffer_init_calloc_y(YMEM char *ad, int size_mem)
{
	if (ad != NULL) {
		heap_y = ad;
		max_memory_y = size_mem;
		total_char_y = 0;
	}
}											/* end init_calloc_y() */


PRAGMA_FORCE_DCUMODE
YMEM void *buffer_calloc_y(int num)
{
	YMEM char *base;
	int size =1;
	int             size_n = size * num;

	/* always align on integer boundary */
	while ((unsigned int) total_char_y % sizeof (int) != 0) {
		heap_y++;
		total_char_y++;
	}

	total_char_y += size_n;
	if (total_char_y > max_memory_y) {
		base = NULL;
	} else {
		base = heap_y;
		buffer_reset((char *) base, 0, size_n);
	}
	heap_y += size_n;

	return base;
}

/****************************/
/* Allocation in extern mem */

PRAGMA_FORCE_DCUMODE
void buffer_init_calloc_extern(char *adr, int size_mem)
{
	if (adr != NULL) {
		heap_extern = adr;
		max_memory_extern = size_mem;
		total_char_extern = 0;
	}
}

PRAGMA_FORCE_DCUMODE
EXTERN void *buffer_calloc_extern(int num)
{
	EXTERN char *base;
	
	int             size_n = num;

	/* always align on integer boundary */
	while ((unsigned int) total_char_extern % sizeof (int) != 0) {
		heap_extern++;
		total_char_extern++;
	}

	total_char_extern += size_n;
	if (total_char_extern > max_memory_extern) {
		base = NULL;
	} else {
		base = heap_extern;
		buffer_reset((char *) base, 0, size_n);
	}
	heap_extern += size_n;

	return base;
}
/****************************/
/* Allocation in esram mem */

PRAGMA_FORCE_DCUMODE
void buffer_init_calloc_esram(char *adr, int size_mem)
{
	if (adr != NULL) {
		heap_esram = adr;
		max_memory_esram = size_mem;
		total_char_esram = 0;
	}
}

PRAGMA_FORCE_DCUMODE
#ifdef __flexcc2__
ESRAM void *buffer_calloc_esram(int num)
{
	ESRAM char *base;
#else
EXTERN void *buffer_calloc_esram(int num)
{
	EXTERN char *base;
#endif

	
	int             size_n = num;

	/* always align on integer boundary */
	while ((unsigned int) total_char_esram % sizeof (int) != 0) {
		heap_esram++;
		total_char_esram++;
	}

	total_char_esram += size_n;
	if (total_char_esram > max_memory_esram) {
		base = NULL;
	} else {
		base = heap_esram;
		buffer_reset((char *) base, 0, size_n);
	}
	heap_esram += size_n;

	return base;
}
