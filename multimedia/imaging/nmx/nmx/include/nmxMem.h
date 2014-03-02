/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmxMem.h
 
	\author   Luigi Arnone luigi.arnone@stericsson.com

*/

#ifndef H_nmxMem
#define H_nmxMem 1

typedef struct nmxMem {
	void    *p;
	void    *pinit;
	int32_t size;
} nmxMem;

void nmxMemInit(nmxMem *mem, uint32_t size, void *pdata);
void *nmxAlloc(nmxMem *mem, uint32_t size);
nmxRCode_t nmxCreateMatrix(nmxMem *mem, nmxMatrix *mat, uint32_t ndims,
	int32_t *dims, int32_t *strides, int32_t etype);

#endif /* H_nmxMem */
