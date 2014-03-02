/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmx.h
 
	\author   Luigi Arnone luigi.arnone@stericsson.com

*/

#ifndef H_nmx
#define H_nmx 1

#define NMX_REVISION_MAJOR 2
#define NMX_REVISION_MINOR 1
#define NMX_REVISION_BUILD 120209

#ifndef NMX_BASE_IDX
# define NMX_BASE_IDX 0
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "nmxDatatype.h"
#include "nmxMem.h"
#include "nmxMathdefs.h"
#include "nmxMacros.h"
#include "nmxError.h"

#include "nmxFunctions.h"

#ifdef __cplusplus
}
#endif

#endif /* H_nmx */

