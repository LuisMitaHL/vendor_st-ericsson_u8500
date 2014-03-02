/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmxErrors.h
 
	\author   Luigi Arnone luigi.arnone@gmail.com

*/

#ifndef H_nmxErrors
#define H_nmxErrors 1

#include "nmxDatatype.h"

#define NMX_RET_ERROR 1
#define NMX_RET_OK 0

#define NMX_STR(x) #x

#define NMX_FAIL(expr,file,line,msg) (printf( \
	"ERROR: %s in file: %s line: %d expresion: %s -> %d\n", \
	msg,file,line,NMX_STR(expr),(expr)))

#define NMX_ASSERT(expr,msg) ((expr) \
	? NMX_RET_OK \
	: (NMX_FAIL(expr, __FILE__, __LINE__, msg), NMX_RET_ERROR))

#ifdef NMX_CHECKS
# define NMX_ASSERT_DBG(expr,msg) NMX_ASSERT(expr,msg)
#else
# define NMX_ASSERT_DBG(expr,msg) NMX_RET_OK
#endif

#endif /* H_nmxErrors */

