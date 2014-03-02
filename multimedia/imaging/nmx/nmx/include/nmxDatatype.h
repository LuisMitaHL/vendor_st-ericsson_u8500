/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*!
	CVS Id    $Id$

	\file	  nmxDatatype.h
 
	\author   Luigi Arnone luigi.arnone@stericsson.com

*/

#ifndef H_nmxDatatype
#define H_nmxDatatype 1

#include <stdint.h>

/*
	Integer datatype:
		we use the datatype defined in stdint.h in the form of
		[u]int{8,16,32,64}_t; e.i.: uint32_t
		those type are standard for C99
*/

typedef char*	nmxPbyte_t;

#define NMX_TYPE_SHIFT 16
#define NMX_SIZE_MASK ((1 << (NMX_TYPE_SHIFT)) - 1)
#define NMX_MAKE_TYPE(type,size) (((type) << (NMX_TYPE_SHIFT)) + (size))
#define NMX_SIZEOF(nmxtype) ((nmxtype) & (NMX_SIZE_MASK))

enum NMX_TYPES {
	NMX_TYPE_NULL = 0,
	NMX_TYPE_U8   = NMX_MAKE_TYPE(0,sizeof(uint8_t)),
	NMX_TYPE_S8   = NMX_MAKE_TYPE(1,sizeof(int8_t)),
	NMX_TYPE_U16  = NMX_MAKE_TYPE(2,sizeof(uint16_t)),
	NMX_TYPE_S16  = NMX_MAKE_TYPE(3,sizeof(int16_t)),
	NMX_TYPE_U32  = NMX_MAKE_TYPE(4,sizeof(uint32_t)),
	NMX_TYPE_S32  = NMX_MAKE_TYPE(5,sizeof(int32_t)),
	NMX_TYPE_U64  = NMX_MAKE_TYPE(6,sizeof(uint64_t)),
	NMX_TYPE_S64  = NMX_MAKE_TYPE(7,sizeof(int64_t)),
	NMX_TYPE_F32  = NMX_MAKE_TYPE(8,sizeof(float)),
	NMX_TYPE_F64  = NMX_MAKE_TYPE(9,sizeof(double))
};

#define NMX_CHECK_M_TYPE(mat,type) ((mat)->etype == (type))
#define NMX_CHECK_MM_TYPE(mat1,mat2) ((mat1)->etype == (mat2)->etype)

#define NMX_NDIMS_MAX 8

typedef uint32_t	nmxRCode_t;
typedef int32_t		nmxStride_t;
typedef uint32_t	nmxSize_t;
typedef uint32_t	nmxEtype_t;
typedef uint32_t	nmxDimension_t;
typedef int32_t		nmxEsize_t;

typedef struct nmxMatrix {
	void		*pdata;
	nmxEtype_t	etype;
	nmxDimension_t	ndims;
	nmxSize_t	dims[NMX_NDIMS_MAX];
	nmxStride_t	strides[NMX_NDIMS_MAX];
} nmxMatrix;

#endif /* H_nmxDatatype */

