//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_asn1_macros.h
//
// 
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_asn1_macros.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Macros specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_SUPL_ASN1_MACROS_H
#define INCLUSION_GUARD_SUPL_ASN1_MACROS_H

//#include "stdio.h"
//#include "malloc.h"

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//internal supl
#include "GN_SUPL_api.h"

/*
 
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/


#define ASN1_CHOICE present
#define ASN1_UNION choice

#define ASN1_BIT_STR_LEN size
#define ASN1_BIT_STR_PTR buf

#define ASN1_OCTET_STR_LEN size
#define ASN1_OCTET_STR_PTR buf

#define ASN1_SEQ_COUNT list.count
#define ASN1_SEQ_PTR list.array
#define ASN1_SEQ_LIST(x) x.list
#define ASN1_SEQ_SIZE(_x, y) (_x->list.size = (y));

#define ASN1_REF(_x, _y) _x->_y
#define ASN1_DEREF(x) x
#define ASN1_DEREF_1(x) &(x)

#define ASN1_PTR_DECLARE(p) **(p)
#define ASN1_PTR_REF(p) &(p)

#define ASN1_ASSIGN_1(x) x
#define ASN1_ASSIGN_2(x) &(x)

#define ASN1_SET(x) *(x)
#define ASN1_SET_CHK(x) if ((x) != NULL) ASN1_SET(x)
#define ASN1_GET(x) *(x)

#define ASN1_SET_ADD(list, ptr) ASN_SET_ADD(list, ptr)

#define ASN1_PARAM_INIT(_x, _y) (ASN1_REF(_x, _y) = NULL)
#define ASN1_PARAM_2_INIT(_x, _y) (_x._y = NULL)
#define ASN1_PARAM_3_INIT(_x, _y) (_x->_y = NULL)
#define ASN1_PARAM_4_INIT(_x, _y) (_x._y = NULL)

#define ASN1_PARAM_SET(_x, _y) //dummy line
#define ASN1_PARAM_2_SET(_x, _y) //dummy line
#define ASN1_PARAM_3_SET(_x, _y) //dummy line
#define ASN1_PARAM_4_SET(_x, _y) //dummy line

#define ASN1_PARAM_ALLOC(x, y) SUPL_HEAP_ALLOC(x, y)

#define ASN1_PARAM_STORE(x, y) (*x) = NULL

#define ASN1_CHK_PARAM_PRESENT(_x, _y) (ASN1_REF(_x, _y) != NULL)
#define ASN1_CHK_PARAM_2_PRESENT(_x, _y) (_x._y != NULL)
#define ASN1_CHK_PARAM_3_PRESENT(_x, _y) (_x->_y != NULL)
#define ASN1_CHK_PARAM_4_PRESENT(_x, _y) (_x._y != NULL)
 

#define SUPL_HEAP_ALLOC(ptr, type) \
  ptr = (type*)GN_Calloc(1,sizeof(type))

#define ASN1_ENUM_SET(_x, y) \
_x.size = 1;\
SUPL_HEAP_ALLOC(_x.buf, uint8_t);\
if (_x.buf != NULL) *(_x.buf) = y

#define ASN1_ENUM_SET_CHK(_x, y) \
if ((_x) != NULL) {\
_x->size = 1;\
SUPL_HEAP_ALLOC(_x->buf, uint8_t);\
if (_x->buf != NULL) *(_x->buf) = y;}

#define ASN1_ENUM_1_GET(_x) _x.buf
#define ASN1_ENUM_2_GET(_x) _x->buf

#define ASN1_UTC_SET(_x, y) \
_x.ASN1_OCTET_STR_LEN = SUPL_TIME_STAMP_SIZE;\
_x.ASN1_OCTET_STR_PTR = (uint8_t *)y

#define ASN1_UTC_GET(_x) (uint8_t *)(_x.ASN1_OCTET_STR_PTR)

#define ASN1_SEQ_1_PTR(x) x->list.array
#define ASN1_SEQ_2_PTR(x) x.list.array
#define ASN1_SEQ_1_COUNT(x) x->list.count
#define ASN1_SEQ_2_COUNT(x) x.list.count

#define ASN1_SEQ_ALLOC(x, y) ASN1_PARAM_ALLOC(x, y)
#define ASN1_SEQ_MEMBER(x) x
#define ASN1_SEQ_ADD(list, ptr) ASN1_SET_ADD(list, ptr)
#define ASN1_SEQ_LIMIT(count, max) (count < max)
#define ASN1_SEQ_NEXT(x) x++
#define ASN1_SEQ_END(ptr, count, max) (count >= max)
#define ASN1_SEQ_VALUE(x) x
#define ASN1_SEQ_SET(ptr, list) ASN1_SEQ_ADD(list, ptr)
#define ASN1_SEQ_SET_LIST(ptr, list) // dummy line
#define ASN1_SEQ_INIT_NEXT(ptr, value) // dummy line

#endif /* INCLUSION_GUARD_SUPL_ASN1_MACROS_H */
