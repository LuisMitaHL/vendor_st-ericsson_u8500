/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**
 * \file 	t1xhv_types.h													   
 * \brief 	global types for mmtc
 * \author  Philippe Rochette
 *
 * Define all basic types.
 ******************************************************************************/


#ifndef _MMTC_TYPES_H_
#define _MMTC_TYPES_H_

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
/* These type definitions are compiler and platform dependent */
/* (from AST) */
typedef  unsigned char       t_uint8; /* 8-bit */
typedef    signed char       t_sint8;

typedef  unsigned short      t_uint16; /* 16-bit container on 32-bits architectures */
typedef    signed short      t_sint16;

typedef  unsigned long       t_uint32; /* 32-bit container on 32-bits architectures */
typedef    signed long       t_sint32;

typedef  unsigned long       t_address;
#ifdef _CYGWIN
typedef  unsigned long long			 t_uint64;
typedef  signed long long			 t_sint64;
#else
typedef  __int64			 t_uint64;
typedef  __int64			 t_sint64;
#endif
#endif /* _MMTC_TYPES_H_ */


