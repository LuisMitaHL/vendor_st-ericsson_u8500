//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_asn1_macros.h
//
//
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_asn1_macros.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Macros specifically for using the RRLP encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef INCLUSION_GUARD_RRLP_ASN1_MACROS_H
#define INCLUSION_GUARD_RRLP_ASN1_MACROS_H

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

#include "supl_asn1_macros.h"
#include "GN_SUPL_api.h"

/*
 **************************************************************************
 * Definitions
 **************************************************************************
*/

#define RRLP_HEAP_ALLOC(ptr, type) \
  ptr = (type*)GN_Calloc(1,sizeof(type))

#undef RRLP_ASN1_ENUM_SET
#define RRLP_ASN1_ENUM_SET(_x, y) \
_x.size = 1;\
RRLP_HEAP_ALLOC(_x.buf, uint8_t);\
if (_x.buf != NULL) *(_x.buf) = y

#undef RRLP_ASN1_ENUM_SET_CHK
#define RRLP_ASN1_ENUM_SET_CHK(_x, y) \
if ((_x) != NULL) {\
_x->size = 1;\
RRLP_HEAP_ALLOC(_x->buf, uint8_t);\
if (_x->buf != NULL) *(_x->buf) = y;}

#undef RRLP_ASN1_PARAM_ALLOC
#define RRLP_ASN1_PARAM_ALLOC(x, y) RRLP_HEAP_ALLOC(x, y)





#endif                // INCLUSION_GUARD_RRLP_ASN1_MACROS_H
